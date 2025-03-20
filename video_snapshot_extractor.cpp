#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

/**
 * VideoSnapshotExtractor - A program that extracts snapshots from video files
 * at specified intervals.
 * 
 * This program uses OpenCV to process video files and extract frames
 * at regular intervals (every 1.5 seconds by default).
 * 
 * Author: Harvard CS Master's Student
 * Date: March 2025
 */
class VideoSnapshotExtractor {
private:
    const double snapshot_interval_seconds; // Interval between snapshots
    const std::string output_directory;    // Directory to save snapshots

public:
    /**
     * Constructor
     * @param interval_seconds Interval between snapshots in seconds
     * @param out_dir Directory to save snapshots, created if it doesn't exist
     */
    VideoSnapshotExtractor(double interval_seconds = 1.5, 
                          const std::string& out_dir = "snapshots") 
        : snapshot_interval_seconds(interval_seconds), output_directory(out_dir) {
        
        // Create output directory if it doesn't exist
        if (!fs::exists(output_directory)) {
            fs::create_directory(output_directory);
            std::cout << "Created output directory: " << output_directory << std::endl;
        }
    }
    
    /**
     * Process a video file and extract snapshots
     * @param video_path Path to the video file
     * @return Number of snapshots extracted, or -1 if an error occurred
     */
    int processVideo(const std::string& video_path) {
        // Open the video file
        cv::VideoCapture video(video_path);
        
        // Check if video opened successfully
        if (!video.isOpened()) {
            std::cerr << "Error: Could not open video file: " << video_path << std::endl;
            return -1;
        }
        
        // Get video properties
        double fps = video.get(cv::CAP_PROP_FPS);
        double frames_between_snapshots = fps * snapshot_interval_seconds;
        double total_frames = video.get(cv::CAP_PROP_FRAME_COUNT);
        
        std::cout << "Video loaded successfully:" << std::endl;
        std::cout << " - FPS: " << fps << std::endl;
        std::cout << " - Total frames: " << total_frames << std::endl;
        std::cout << " - Duration: " << total_frames / fps << " seconds" << std::endl;
        std::cout << " - Extracting snapshots every " << snapshot_interval_seconds 
                  << " seconds (" << frames_between_snapshots << " frames)" << std::endl;
        
        // Get video filename without extension for naming snapshots
        fs::path video_file_path(video_path);
        std::string video_filename = video_file_path.stem().string();
        
        // Process the video
        cv::Mat frame;
        int frame_count = 0;
        int snapshot_count = 0;
        
        while (video.read(frame)) {
            // Take snapshot at specified intervals
            if (frame_count % static_cast<int>(frames_between_snapshots) == 0) {
                // Generate timestamp for the snapshot
                double time_seconds = frame_count / fps;
                std::string timestamp = formatTimestamp(time_seconds);
                
                // Create snapshot filename
                std::string snapshot_filename = output_directory + "/" + 
                                               video_filename + "_" + 
                                               timestamp + ".jpg";
                
                // Save the snapshot
                cv::imwrite(snapshot_filename, frame);
                
                std::cout << "Saved snapshot " << (snapshot_count + 1) 
                          << " at " << timestamp << std::endl;
                
                snapshot_count++;
            }
            
            frame_count++;
        }
        
        std::cout << "Completed processing video: " << video_path << std::endl;
        std::cout << "Extracted " << snapshot_count << " snapshots" << std::endl;
        
        // Release the video
        video.release();
        
        return snapshot_count;
    }

private:
    /**
     * Format a seconds value to a readable timestamp (HH:MM:SS.ms)
     * @param seconds Time in seconds
     * @return Formatted timestamp string
     */
    std::string formatTimestamp(double seconds) {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        int milliseconds = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << hours << "_"
           << std::setfill('0') << std::setw(2) << minutes << "_"
           << std::setfill('0') << std::setw(2) << secs << "_"
           << std::setfill('0') << std::setw(3) << milliseconds;
           
        return ss.str();
    }
};

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    std::string video_path;
    
    // Check if a video path was provided as a command-line argument
    if (argc > 1) {
        video_path = argv[1];
    } else {
        // No video path provided, prompt user for input
        std::cout << "Enter the path to the video file: ";
        std::getline(std::cin, video_path);
    }
    
    // Create extractor with default settings (1.5 second interval)
    VideoSnapshotExtractor extractor;
    
    // Process the video
    int result = extractor.processVideo(video_path);
    
    return (result >= 0) ? 0 : 1;
}
